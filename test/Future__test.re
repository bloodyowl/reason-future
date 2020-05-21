open TestFramework;

describe("Future", ({test, testAsync}) => {
  test("Future.make runs synchronously", ({expect}) => {
    let ran = ref(false);
    let _ =
      Future.make(resolve => {
        ran := true;
        resolve(() => 1);
        None;
      });
    expect.bool(ran.contents).toBeTrue();
  });

  test("Future.get", ({expect}) => {
    Future.value(1)->Future.get(value => expect.int(value).toBe(1))
  });

  test("Future.map", ({expect}) => {
    Future.value(1)
    ->Future.map(value => value + 2)
    ->Future.get(value => expect.int(value).toBe(3))
  });

  test("Future.flatMap", ({expect}) => {
    Future.value(1)
    ->Future.map(value => value + 2)
    ->Future.flatMap(value => Future.value(value + 2))
    ->Future.get(value => expect.int(value).toBe(5))
  });

  testAsync("Future.cancel", ({expect, callback}) => {
    let counter = ref(0);
    let future =
      Future.make(resolve => {
        let timeoutId = Js.Global.setTimeout(() => resolve(() => 1), 10);
        Some(() => Js.Global.clearTimeout(timeoutId));
      });
    future->Future.get(_ => {incr(counter)});
    Future.cancel(future);
    let _ =
      Js.Global.setTimeout(
        () => {
          expect.int(counter.contents).toBe(0);
          callback();
        },
        20,
      );
    ();
  });

  testAsync("Future.cancel deep", ({expect, callback}) => {
    let counter = ref(0);
    let future =
      Future.make(resolve => {
        let timeoutId = Js.Global.setTimeout(() => resolve(() => 1), 10);
        Some(() => Js.Global.clearTimeout(timeoutId));
      })
      ->Future.map(x => {
          incr(counter);
          x + 1;
        })
      ->Future.flatMap(x => {
          incr(counter);
          Future.value(x + 1);
        });
    future->Future.get(_ => {incr(counter)});
    Future.cancel(future);
    let _ =
      Js.Global.setTimeout(
        () => {
          expect.int(counter.contents).toBe(0);
          callback();
        },
        20,
      );
    ();
  });

  test("Future.all2", ({expect}) => {
    Future.all2(Future.value(1), Future.value(2))
    ->Future.get(value => expect.value(value).toEqual((1, 2)))
  });

  test("Future.all3", ({expect}) => {
    Future.all3(Future.value(1), Future.value(2), Future.value(3))
    ->Future.get(value => expect.value(value).toEqual((1, 2, 3)))
  });

  test("Future.all4", ({expect}) => {
    Future.all4(
      Future.value(1),
      Future.value(2),
      Future.value(3),
      Future.value(4),
    )
    ->Future.get(value => expect.value(value).toEqual((1, 2, 3, 4)))
  });

  test("Future.all5", ({expect}) => {
    Future.all5(
      Future.value(1),
      Future.value(2),
      Future.value(3),
      Future.value(4),
      Future.value(5),
    )
    ->Future.get(value => expect.value(value).toEqual((1, 2, 3, 4, 5)))
  });

  test("Future.all", ({expect}) => {
    Future.all([|
      Future.value(1),
      Future.value(2),
      Future.value(3),
      Future.value(4),
      Future.value(5),
    |])
    ->Future.get(value => expect.value(value).toEqual([|1, 2, 3, 4, 5|]))
  });
});

describe("FutureResult", ({test}) => {
  test("FutureResult.mapOk", ({expect}) => {
    Future.value(Ok(1))
    ->FutureResult.mapOk(x => x + 2)
    ->FutureResult.getOk(value => expect.int(value).toBe(3))
  });
  test("FutureResult.flatMapOk", ({expect}) => {
    Future.value(Ok(1))
    ->FutureResult.flatMapOk(x => Future.value(Ok(x + 2)))
    ->FutureResult.getOk(value => expect.int(value).toBe(3))
  });
  test("FutureResult.mapError", ({expect}) => {
    Future.value(Error(1))
    ->FutureResult.mapError(x => x + 2)
    ->FutureResult.getError(value => expect.int(value).toBe(3))
  });
  test("FutureResult.flatMapError", ({expect}) => {
    Future.value(Error(1))
    ->FutureResult.flatMapError(x => Future.value(Error(x + 2)))
    ->FutureResult.getError(value => expect.int(value).toBe(3))
  });
});

describe("FuturePromise", ({testAsync}) => {
  testAsync("FuturePromise.fromPromise ok", ({expect, callback}) => {
    FuturePromise.fromPromise(Js.Promise.resolve(1))
    ->Future.get(value => {
        expect.value(value).toEqual(Ok(1));
        callback();
      })
  });
  testAsync("FuturePromise.fromPromise error", ({expect, callback}) => {
    FuturePromise.fromPromise(Js.Promise.reject(1->Obj.magic))
    ->Future.get(value => {
        expect.value(value).toEqual(Error(1));
        callback();
      })
  });
  testAsync("FuturePromise.fromPromiseUnsafe", ({expect, callback}) => {
    FuturePromise.fromPromiseUnsafe(Js.Promise.resolve(1))
    ->Future.get(value => {
        expect.int(value).toBe(1);
        callback();
      })
  });
  testAsync("FuturePromise.toPromise", ({expect, callback}) => {
    let _ =
      FuturePromise.toPromise(Future.value(1))
      |> Js.Promise.then_(value => {
           expect.int(value).toBe(1);
           callback();
           Js.Promise.resolve();
         });
    ();
  });
  testAsync("FuturePromise.toPromiseResult ok", ({expect, callback}) => {
    let _ =
      FuturePromise.toPromiseResult(Future.value(Ok(1)))
      |> Js.Promise.then_(value => {
           expect.int(value).toBe(1);
           callback();
           Js.Promise.resolve();
         });
    ();
  });
  testAsync("FuturePromise.toPromiseResult error", ({expect, callback}) => {
    let _ =
      FuturePromise.toPromiseResult(Future.value(Error(1)))
      |> Js.Promise.catch(value => {
           expect.int(value->Obj.magic).toBe(1);
           callback();
           Js.Promise.resolve();
         });
    ();
  });
});
