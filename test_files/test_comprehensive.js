// This is a comprehensive test file of all features that the Republic runtime
// offers. This will be expand upon as time goes on, since running only one
// script compared to the multiple other scripts to test Republic is just
// easier.


function test_0() {

    console.log("Running test 0");

    // Set some variable
    const a = 50;
    const b = 40;
    const c = a + b;
    const d = 90;

    // Test the result
    console.assert(c == d);

    console.log("Test 0 passed");

}

function test_1() {
    console.log("Running test 1");

    const obj = {
        a: 50, // Comment in object
        b: 90,
        c: 140,
    };
    const a = obj.a + obj.b;

    console.assert(a == obj.c);
    console.log("Test 1 passed");
}

test_0();
test_1();

