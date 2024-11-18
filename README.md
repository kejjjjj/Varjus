## A scripting language

Example code

```js
fn ass()
{
	return 5;
}

fn testFunc(val)
{
	let assWrapper = fn() { return ass(); };

	let test = val;
	return {  
		data: 
			fn(callable, a, b) { 
				return callable(a, b) + test;  
			},
		wrapper:
			(test < 5) ? assWrapper : fn() { return test + 7; }
	};
}

fn handleFunc(value)
{
	let obj = testFunc(value);

	let part1 = obj.data(fn(a, b) { return a + b; }, 7, 5);
	let part2 = obj.wrapper();

	return part1 + part2;
}

fn main() {

	let a = handleFunc(4);
	let b = handleFunc(5);
}
```
Output:

```
21: int
29: int
```

