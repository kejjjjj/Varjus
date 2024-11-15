## A scripting language

Example code

```js
fn addArrays(a, b, target)
{
	let i = 0;
	while(i < a.length){
		target[i] = a[i] + b[i];
		i = i + 1;
	}

	return true;
}

fn getMap()
{
	return { 
		data: { 
			addFunc: addArrays,
			keys: [
				{ a: 0 },
				{ a: 1 },
				{ a: 2 }
			],
			keys2: { 
				data: [ 
					{ a: 4 },
					{ a: 5 },
					{ a: 6 } 
				]
			}	
		},
		result: [undefined, undefined, undefined]
	};
}
fn replaceKeys(keyArray, targetValues)
{
	let i = 0;
	while(i < keyArray.length){
		keyArray[i] = targetValues[i];
		i = i + 1;
	}
}
fn main() {

	let map = getMap();
	
	replaceKeys(map.data.keys, [10, 20, 30]);
	replaceKeys(map.data.keys2.data, [40, 50, 60]);

	let target = map.result;
	let addedData = map.data.addFunc(map.data.keys, map.data.keys2.data, target);
}
```
Output:

```
{ data: { addFunc: 11cc84ea0320, keys: [ 10, 20, 30 ], keys2: { data: [ 40, 50, 60 ] } }, result: [ 50, 70, 90 ] }: object
[ 50, 70, 90 ]: array
true: boolean
```

