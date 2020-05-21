type iterator;
type stop;

[@bs.send] external next: (iterator, 'a) => stop = "next";
[@bs.get] external isDone: stop => bool = "done";
[@bs.get] external value: stop => Future.t('a) = "value";
[@bs.get] external endValue: stop => 'a = "value";

let asyncBlock = generator => {
  let block = generator();
  let rec f = future => {
    future->Future.flatMap(x => {
      let next = block->next(x);
      next->isDone ? Future.value(next->endValue) : f(next->value);
    });
  };
  f(Future.value());
};

/**
 *
 * async {
 *   let a = await Future.value(1);
 *   let b = await Future.value(2);
 *   let c = await Future.make(resolve => {Js.Global.setTimeout(() => resolve( 2)); None});
 *   Js.log(a + b + c);
 *   a + b + c;
 * }
 *
 * async {
 *   let a = await Future.value(1);
 *   let b = await Future.value(2);
 *   let c = await Future.make(resolve => {Js.Global.setTimeout(() => resolve(2)); None});
 *   Js.log(a + b + c);
 *   Future.value(a + b + c);
 * }
 *
 * async {
 *   let a = await Future.value(1);
 *   let b = await Future.value(2);
 *   let c = await Future.make(resolve => {Js.Global.setTimeout(() => resolve(2)); None});
 *   Js.log(a + b + c);
 *   async Future.value(a + b + c);
 * }
 *
 */
[%bs.raw
  {|
  asyncBlock(function *() {
    let a = yield Future.value(1);
    let b = yield Future.value(2);
    let c = yield Future.make(resolve => setTimeout(() => resolve(2)));
    console.log(a + b + c);
    return (a + b + c)
  })
|}
]
->FutureDebug.tap(Js.log2("Return plain value"));

[%bs.raw
  {|
  asyncBlock(function *() {
    let a = yield Future.value(1);
    let b = yield Future.value(2);
    let c = yield Future.make(resolve => setTimeout(() => resolve(2)));
    console.log(a + b + c);
    return Future.value(a + b + c)
  })
|}
]
->FutureDebug.tap(Js.log2("Return future"));

[%bs.raw
  {|
  asyncBlock(function *() {
    let a = yield Future.value(1);
    let b = yield Future.value(2);
    let c = yield Future.make(resolve => setTimeout(() => resolve(2)));
    console.log(a + b + c);
    return yield Future.value(a + b + c)
  })
|}
]
->FutureDebug.tap(Js.log2("Return await future"));
