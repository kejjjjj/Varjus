## A scripting language

## Implemented
of course there's a LOT more happening behind the scenes, but here's a quick summary
- built-in types
  - undefined
  - boolean
  - int
  - double
  - string
  - callable
  - array
  - object
  - lambda
- statements
  - if
  - else if
  - else
  - while
  - for
  - continue
  - break
  - return
- 31 operators
- global variables
- local variables
- imports
- exports
- modules
- exceptions
  - try
  - catch
  - throw
- aggregate types
- variable declarations
  - let
- functions
  - closures
  - hoisting
- methods and members
  - array (.length and  e.g. .push(value))
- postfix
  - subscript []
  - member access .
  - function calls ()
  - increment ++
  - decrement --
- unary
  - negation -
  - increment ++
  - decrement --
  - logical not !
- value types
  - copy
  - reference


### Types
```js
let undefinedValue = undefined;
let booleanValue = true;
let intValue = 1;
let doubleValue = 1.5;
let stringValue = "asdasd";
let arrayValue = [ 1, 2, 3 ];
let objectValue = { a: 1, b: 2, c: 3 };
let lambdaValue = fn(a, b) { return a + b; };
```

### Functions
```js
fn add(param1, param2)
{
     return param1 + param2;
}

// entry point
fn main()
{
    let sum = add(3, 7);
    let concat = add("hello ", "world!");
    return [sum, concat];
}
```

### Arrays
Arrays are treated as references
```js
let array = [ 1, 2, 3 ];
let length = array.length; // 3
array.push(4); // array is now [ 1, 2, 3, 4 ]

let arrayReference = array; // arrayReference shares ownership with array
arrayReference[0] = 1; // this will also update the value of array[0] to 1

//find even values
array.filter( fn(value) { return value % 2 == 0; } );  //returns [2, 4]

```

### Objects
Objects are treated as references
```js
let object = { key: "value", key2: 2 };
let key = object.key; // "value"
let key2 = object.key2; // 2
```

### Lambdas
Lambdas are treated as references
```js
let lambda = fn(arg1, arg2) {
  return arg1 + arg2;
};

let result = lambda(1, 2); // 3
```

  - #### Closures
    ```js
	fn closureTest()
	{
	  let capturedVariable = 5;
   	  //this lambda will capture the state of capturedVariable, 
	  //so that it doesn't get freed when this function ends
      return fn() {
        return capturedVariable; 
      }
    }

	let getLambda = closureTest(); // get the lambda
	let result = getLambda(); // this will return 5
    ```

	
### Function hoisting
The order in which functions are declared does not matter

```js
fn main(){
   return functionBelowMain();
}

fn functionBelowMain(){
   return "hello";
}
```
Though it should be noted that this is ONLY for functions
```js
fn main(){
   return variableBelowMain; //the interpreter does not know what variableBelowMain is at this point
}

let variableBelowMain = "hello";

```

### Exceptions

```js
fn expectShortString(inputString)
{

	try{
		if(inputString.length < 6)
			throw "expected > 5 characters";

		return "success";

	} catch(error){
		return error;
	}
}

fn main() {
	return {
		a: expectShortString("hello world"), // "success"
		b: expectShortString("hello"), // "expected > 5 characters"
	};
}
```

## Credits
- [Catch2 (Unit Testing)](https://github.com/catchorg/Catch2/)
