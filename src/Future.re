// Resolving the value with unit => 'value instead of 'value
// is there so that there might be an opportunity for error
// throwing in the future.
type value('value) = unit => 'value;

type status('value) =
  | Pending
  | Cancelled
  | Done(value('value));

[@unboxed]
type cancellationToken =
  | Cancel(unit => unit);

type cancelFunction = unit => unit;

type futureCallback('value) = (unit => 'value) => unit;

type resolve('value) = value('value) => unit;

type setup('value) = resolve('value) => option(cancelFunction);

[@unboxed]
type t('value) =
  | Future(futureCallback('value) => cancellationToken);

let noop = _ => ();

let make = (setup: setup('value)): t('value) => {
  let status = ref(Pending);
  let subscriptions = ref(None);
  let maybeCancel =
    setup(resolvedValueGetter => {
      switch (status.contents) {
      | Pending =>
        status := Done(resolvedValueGetter);
        switch (subscriptions.contents) {
        | None => ()
        | Some(callbacks) =>
          callbacks->Js.Array2.forEach(cb => cb(resolvedValueGetter));
          subscriptions := None;
        };
      | Cancelled
      | Done(_) => ()
      }
    });
  Future(
    getFunc => {
      switch (status.contents) {
      | Done(resolvedValueGetter) =>
        getFunc(resolvedValueGetter);
        Cancel(noop);
      | Cancelled => Cancel(noop)
      | Pending =>
        subscriptions :=
          (
            switch (subscriptions.contents) {
            | Some(array) =>
              let _ = array->Js.Array2.push(getFunc);
              Some(array);
            | None => Some([|getFunc|])
            }
          );
        Cancel(
          () => {
            subscriptions := None;
            switch (maybeCancel) {
            | Some(cancel) => cancel()
            | None => ()
            };
            status := Cancelled;
          },
        );
      }
    },
  );
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
    resolve(() => value);
    None;
  });

let get = (Future(getFunc), getter) => {
  let _ = getFunc(getVal => getter(getVal()));
  ();
};

let cancel = (Future(getFunc)) => {
  let Cancel(cancel) = getFunc(_ => ());
  cancel();
};

let map = (Future(getFunc), mapper) => {
  make(resolve => {
    let Cancel(cancel) =
      getFunc(getVal => {resolve(() => mapper(getVal()))});
    Some(() => cancel());
  });
};

let flatMap = (Future(getFunc), mapper) => {
  make(resolve => {
    let Cancel(cancel) =
      getFunc(getVal1 => {
        let Future(getFunc2) = mapper(getVal1());
        let _ = getFunc2(getVal2 => {resolve(() => getVal2())});
        ();
      });
    Some(() => {cancel()});
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
  all2(all4(a, b, c, d), e)
  ->map((((a, b, c, d), e)) => (a, b, c, d, e)) /* testAll->get(Js.log2("All"))*/;
};
