#include "stdfs.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"

#include "fs/fs_globals.hpp"

#include "runtime/misc/defs.hpp"

#include <fstream>
#include <filesystem>

using namespace Varjus;

FORWARD_DECLARE_METHOD(FsRead);
FORWARD_DECLARE_METHOD(FsWrite);
FORWARD_DECLARE_METHOD(FsAppend);
FORWARD_DECLARE_METHOD(FsExists);
FORWARD_DECLARE_METHOD(FsDelete);
FORWARD_DECLARE_METHOD(FsListDirectory);

void CStdFsValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("read"),           FsRead,          1);
	receiver.AddMethod(VSL("write"),          FsWrite,         2);
	receiver.AddMethod(VSL("append"),         FsAppend,        2);
	receiver.AddMethod(VSL("exists"),         FsExists,        1);
	receiver.AddMethod(VSL("delete"),         FsDelete,        1);
	receiver.AddMethod(VSL("list_directory"), FsListDirectory, 1);

}

DEFINE_METHOD(FsRead, args)
{
	const auto path = args[0];

	if (path->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.read expected a string, but got \"{}\""), path->TypeAsString()));
	}

	//doesn't exist, return undefined
	if (!fs::file_exists(path->ToString()))
		return IValue::Construct(ctx->m_pRuntime);

#ifdef UNICODE
	std::fstream stream(LocaleConverter::u16string_to_ansi(path->ToString()), 
		static_cast<std::ios_base::openmode>(std::ios_base::in | std::ios_base::binary));
#else
	std::fstream stream(path->ToString(), static_cast<std::ios_base::openmode>(std::ios_base::in | std::ios_base::binary));
#endif

	std::istreambuf_iterator<char> begin(stream), end;
	return CStringValue::Construct(ctx->m_pRuntime, VarjusString(begin, end));
}
DEFINE_METHOD(FsWrite, args)
{
	const auto path = args[0];
	const auto data = args[1];

	if (path->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.write 1. expected a string, but got \"{}\""), path->TypeAsString()));
	}
	if (data->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.write 2. expected a string, but got \"{}\""), data->TypeAsString()));
	}

#ifdef UNICODE
	std::fstream stream(LocaleConverter::u16string_to_ansi(path->ToString()),
		static_cast<std::ios_base::openmode>(std::ios_base::out | std::ios_base::binary));

	stream << LocaleConverter::u16string_to_ansi(data->ToString());
#else
	std::fstream stream(path->ToString(), static_cast<std::ios_base::openmode>(std::ios_base::out | std::ios_base::binary));
	stream << data->ToString();
#endif
	return IValue::Construct(ctx->m_pRuntime);
}
DEFINE_METHOD(FsAppend, args)
{
	const auto path = args[0];
	const auto data = args[1];

	if (path->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.append 1. expected a string, but got \"{}\""), path->TypeAsString()));
	}
	if (data->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.append 2. expected a string, but got \"{}\""), data->TypeAsString()));
	}

#ifdef UNICODE
	std::fstream stream(LocaleConverter::u16string_to_ansi(path->ToString()),
		static_cast<std::ios_base::openmode>(std::ios_base::app | std::ios_base::binary));

	stream << LocaleConverter::u16string_to_ansi(data->ToString());

#else
	std::fstream stream(path->ToString(), static_cast<std::ios_base::openmode>(std::ios_base::app | std::ios_base::binary));
	stream << data->ToString();
#endif
	return IValue::Construct(ctx->m_pRuntime);
}

DEFINE_METHOD(FsExists, args)
{
	const auto path = args[0];

	if (path->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.exists expected a string, but got \"{}\""), path->TypeAsString()));
	}

	return CBooleanValue::Construct(ctx->m_pRuntime, fs::file_exists(path->ToString()));
}

DEFINE_METHOD(FsDelete, args)
{
	const auto path = args[0];

	if (path->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.delete expected a string, but got \"{}\""), path->TypeAsString()));
	}

	return CBooleanValue::Construct(ctx->m_pRuntime, std::filesystem::remove(path->ToString()));
}

DEFINE_METHOD(FsListDirectory, args)
{
	const auto path = args[0];

	if (path->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("fs.list_directory expected a string, but got \"{}\""), path->TypeAsString()));
	}

	IValues arrValues;

	for (auto& entry : std::filesystem::directory_iterator(path->ToString())) {

#ifdef UNICODE
		const auto _path = CStringValue::Construct(ctx->m_pRuntime, entry.path().u16string());
#else
		const auto _path = CStringValue::Construct(ctx->m_pRuntime, entry.path().string());
#endif
		const auto typeStr = entry.is_directory() ? VSL("d") : VSL("f");
		const auto type = CStringValue::Construct(ctx->m_pRuntime, typeStr);

		__ObjectValues properties;
		properties.push_back({ CStringValue::Construct(ctx->m_pRuntime, VSL("path")), _path });
		properties.push_back({ CStringValue::Construct(ctx->m_pRuntime, VSL("type")), type });

		auto obj = CObjectValue::Construct(ctx->m_pRuntime, std::move(properties));

		arrValues.push_back(obj);
	}

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(arrValues));
}