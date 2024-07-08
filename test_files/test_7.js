
// A bit more complicated testing using multiple different features.
const test_1 = {
    a: 20,
    b: 40,
    answer: 60,
};
const got_1 = test_1.a + test_1.b;
console.assert(got_1 == test_1.answer);


const test_2 = {
    hfjdfhjkslfhjksd: 50,
    bjkgdfhgjkrhjske: 60,
    answer: 110,
};
const got_2 = test_2.hfjdfhjkslfhjksd + test_2.bjkgdfhgjkrhjske;
console.assert(got_2 == test_2.answer);


console.assert(0 == 0);

