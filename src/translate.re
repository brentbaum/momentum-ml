let colemakCodes = [|
  30,
  48,
  46,
  34,
  37,
  18,
  20,
  35,
  38,
  21,
  49,
  22,
  50,
  36,
  39,
  19,
  16,
  31,
  32,
  33,
  23,
  4,
  17,
  45,
  24,
  44,
  51,
  52,
  53,
  57,
  12,
  13
|];

let rec find = (a, x, n) =>
  if (a[n] == x) {
    n
  } else if (n + 1 < Array.length(a)) {
    find(a, x, n + 1)
  } else {
    (-1)
  };

let getCharacter = (code) =>
  if (code < 10) {
    string_of_int(code - 1)
  } else if (code === 14) {
    "<-"
  } else if (code === 57) {
    " "
  } else if (code === 28) {
    "->"
  } else {
    let index = find(colemakCodes, code, 0);
    if (index > (-1)) {
      Js.String.get("abcdefghijklmnopqrstuvwxyz,./ -=", index)
    } else {
      string_of_int(code)
    }
  };