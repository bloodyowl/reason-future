let tap = (future, f) => {
  future->Future.get(f);
  future;
};

let tapOk = (future, f) => {
  future->Future.get(x =>
    switch (x) {
    | Ok(x) => f(x)
    | Error(_) => ()
    }
  );
  future;
};

let tapError = (future, f) => {
  future->Future.get(x =>
    switch (x) {
    | Ok(_) => ()
    | Error(x) => f(x)
    }
  );
  future;
};
