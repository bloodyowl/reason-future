type status('value) =
  | Pending(array('value => unit))
  | Cancelled
  | Done('value);

[@unboxed]
type cancellationToken =
  | Cancel(unit => unit);

type cancelFunction = unit => unit;

type futureCallback('value) = 'value => unit;

type resolve('value) = 'value => unit;

type setup('value) = resolve('value) => option(cancelFunction);

[@unboxed]
type t('value) =
  | Future(futureCallback('value) => cancellationToken);

let noop = _ => ();

// [@bs.set]
// external setToString:
//   (futureCallback('value) => cancellationToken, unit => string) => unit =
//   "toString";

// let toString = (status, ()) => {
//   let status =
//     switch (status.contents) {
//     | Pending(callbacks) =>
//       let length = Js.Array2.length(callbacks);
//       {j|Pending($length callbacks)|j};
//     | Cancelled => "Cancelled"
//     | Done(value) => {j|Done($value)|j}
//     };
//   {j|[Future($status)]|j};
// };
let pending = [||];
let running = ref(false);

let rec run = () =>
  if (pending->Js.Array2.length > 0 && !running.contents) {
    switch (pending->Js.Array2.pop) {
    | Some(cb) =>
      running := true;
      cb();
      running := false;
      run();
    | None => ()
    };
  };

let runCallback = cb => {
  let _ = pending->Js.Array2.unshift(cb);
  run();
};

let make = (setup: setup('value)): t('value) => {
  let status = ref(Pending([||]));
  let maybeCancel =
    setup(value => {
      switch (status.contents) {
      | Pending(subscriptions) =>
        status := Done(value);
        subscriptions->Js.Array2.forEach(cb => runCallback(() => cb(value)));
      | Cancelled
      | Done(_) => ()
      }
    });
  let futureGet = cb => {
    switch (status.contents) {
    | Done(value) =>
      runCallback(() => cb(value));
      Cancel(noop);
    | Cancelled => Cancel(noop)
    | Pending(subscriptions) =>
      let _ = subscriptions->Js.Array2.push(cb);
      Cancel(
        () => {
          switch (maybeCancel) {
          | Some(cancel) => cancel()
          | None => ()
          };
          status := Cancelled;
        },
      );
    };
  };
  // futureGet->setToString(toString(status));
  Future(futureGet);
};

type deferred('value) = {
  future: t('value),
  resolve: resolve('value),
};

let deferred = () => {
  let resolveRef = ref(noop);
  {
    future:
      make(resolve => {
        resolveRef := resolve;
        Some(() => resolveRef := noop);
      }),
    resolve: resolveRef.contents,
  };
};

let value = value =>
  make(resolve => {
    resolve(value);
    None;
  });

let get = (Future(getFunc), getter) => {
  let _ = getFunc(value => getter(value));
  ();
};

let cancel = (Future(getFunc)) => {
  let Cancel(cancel) = getFunc(_ => ());
  cancel();
};

let map = (Future(getFunc), ~propagateCancel=true, mapper) => {
  make(resolve => {
    let Cancel(cancel) = getFunc(getVal => {resolve(mapper(getVal))});
    if (propagateCancel) {
      Some(() => cancel());
    } else {
      None;
    };
  });
};

let flatMap = (Future(getFunc), ~propagateCancel=true, mapper) => {
  make(resolve => {
    let Cancel(cancel) =
      getFunc(val1 => {
        let Future(getFunc2) = mapper(val1);
        let _ = getFunc2(val2 => {resolve(val2)});
        ();
      });
    if (propagateCancel) {
      Some(() => cancel());
    } else {
      None;
    };
  });
};

let all = array => {
  array->Belt.Array.reduce(value([||]), (acc, item) => {
    item->flatMap(value => acc->map(acc => acc->Belt.Array.concat([|value|])))
  });
};

let all2 = (a, b) => {
  a->flatMap(a => b->map(b => (a, b)));
};

let all3 = (a, b, c) => {
  all2(all2(a, b), c)->map((((a, b), c)) => (a, b, c));
};

let all4 = (a, b, c, d) => {
  all2(all3(a, b, c), d)->map((((a, b, c), d)) => (a, b, c, d));
};

let all5 = (a, b, c, d, e) => {
  all2(all4(a, b, c, d), e)->map((((a, b, c, d), e)) => (a, b, c, d, e));
};

module Result = {
  let fromResultArray = results => {
    Belt.(
      results[0]
      ->Option.map(item =>
          switch (item) {
          | Ok(value) => Ok([|value|])
          | Error(_) as error => error
          }
        )
      ->Option.map(first => {
          results
          ->Array.sliceToEnd(1)
          ->Array.reduce(first, (acc, item) => {
              switch (acc, item) {
              | (Ok(acc), Ok(item)) => Ok(Array.concat(acc, [|item|]))
              | (Error(_) as error, _)
              | (_, Error(_) as error) => error
              }
            })
        })
      ->Option.getWithDefault(Ok([||]))
    );
  };
};
