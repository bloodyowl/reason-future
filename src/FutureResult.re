let getOk = (future, f) => {
  future->Future.get(x => {
    switch (x) {
    | Ok(x) => f(x)
    | Error(_) => ()
    }
  });
};

let mapOk = (future, f) => {
  future->Future.map(x =>
    switch (x) {
    | Ok(x) => Ok(f(x))
    | Error(_) as x => x
    }
  );
};

let flatMapOk = (future, f) => {
  future->Future.flatMap(x =>
    switch (x) {
    | Ok(x) => f(x)
    | Error(_) as x => Future.value(x)
    }
  );
};

let getError = (future, f) => {
  future->Future.get(x => {
    switch (x) {
    | Ok(_) => ()
    | Error(x) => f(x)
    }
  });
};

let mapError = (future, f) => {
  future->Future.map(x =>
    switch (x) {
    | Ok(_) as x => x
    | Error(x) => Error(f(x))
    }
  );
};

let flatMapError = (future, f) => {
  future->Future.flatMap(x =>
    switch (x) {
    | Ok(_) as x => Future.value(x)
    | Error(x) => f(x)
    }
  );
};
