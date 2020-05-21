let tap = future => {
  future->Future.get(Js.log);
  future;
};

let tapOk = future => {
  future->Future.get(x =>
    switch (x) {
    | Ok(x) => Js.log(x)
    | Error(_) => ()
    }
  );
  future;
};

let tapError = future => {
  future->Future.get(x =>
    switch (x) {
    | Ok(x) => Js.log(x)
    | Error(_) => ()
    }
  );
  future;
};
