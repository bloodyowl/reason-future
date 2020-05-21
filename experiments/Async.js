var { make, get, cancel, all2 } = require("../src/Future.bs.js");

function asyncBlock(blockGenerator) {
  return make(function (resolve) {
    let block = blockGenerator();
    let cancelled = false;
    let cancel;

    function run(value) {
      let next;
      try {
        next = block.next(value);
        if (next.done) {
          let finalValue = next.value;
          resolve(() => finalValue);
          return
        }
      } catch (err) {
        let handled = false;
        resolve(() => {
          handled = true;
          throw err;
        });
        if (!handled) {
          console.error(err)
        }
        return
      };
      cancel = next.value(function (nextValue) {
        if (!cancelled) {
          run(nextValue)
        };
      });
    }

    run();

    return () => {
      cancelled = true;
      if (cancel) cancel();
    }
  })
};

function await_(fn) {
  return fn();
};

function delay(x) {
  return make(function (resolve) {
    let timeoutId = setTimeout(() => {
      let result = x * 2;
      resolve(() => result);
    }, 1000);
    return () => {
      resolve = _ => { };
      clearTimeout(timeoutId);
    }
  })
};

function wrappedDelay(x) {
  return asyncBlock(function* () {
    return await_(yield delay(x));
  })
}

function f(x) {
  return asyncBlock(function* () {
    let a = await_(yield wrappedDelay(x))
    console.log("f1", a);
    let b = await_(yield wrappedDelay(a))
    console.log("f2", b)
    return b;
  })
}


function useEffect(cb) {
  let cancel = cb();
  // setTimeout(() => {
  //   cancel();
  // }, 1500)
}

function setState(state) {
  console.log("setState", state)
}

let x = f(1);

useEffect(function () {
  let future = asyncBlock(function* () {
    let state = await_(yield x);
    setState(state)
  });
  return () => cancel(future);
});

let _ = function () {
  let future = asyncBlock(function* () {
    let state = await_(yield x);
    setState(state)
  });
  return () => cancel(future);
}();

function wrappedDelayThatThrows(x) {
  return asyncBlock(function* () {
    let y = await_(yield delay(x));
    throw new Error("Not_found");
    return y;
  })
}

function fWithException(x) {
  return asyncBlock(function* () {
    try {
      let a = await_(yield wrappedDelayThatThrows(x));
      console.log("hellooo", a.toString())
      return a
    } catch (err) {
      console.log("an error occured");
      return 1
    }
  })
}

fWithException(1);

function fWithBlock(x) {
  console.log(1);
  return asyncBlock(function* () {
    return await_(yield delay(x))
  })
}

let fWithBlock2 = (x) => {
  console.log(1);
  asyncBlock(function* () {
    let value = await_(yield delay(x))
    console.log("fWithBlock2", value);
    return
  })
  console.log(2);
};

fWithBlock2(1);


let __ = asyncBlock(function* () {
  let [a, b] = await_(yield all2(delay(1), delay(2)));
  console.log("all2", a, b);
});

// let ___ = asyncBlock(function* () {
//   let [a, b] = await_(yield invalidCall(delay(1), delay(2)));
//   console.log("all2", a, b);
// });

let asyncValue = asyncBlock(function* () {
  let x = await_(yield make(function (resolve) {
    resolve(() => 1);
  }));
  let y = await_(yield asyncBlock(function* () {
    let y = await_(yield delay(1));
    let z = await_(yield delay(x));
    return y + z;
  }));
  console.log("nested", y);
  return y
});

cancel(asyncValue)

let asyncLoop = asyncBlock(function* () {
  let acc = { contents: 0 };
  for (let i = 0; i <= 10; ++i) {
    acc.contents = acc.contents + await_(yield delay(i));
  };
  console.log("Async loop", acc);
});
