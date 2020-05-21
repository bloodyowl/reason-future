external anyToExn: 'a => exn = "%identity";
external errorToAny: Js.Promise.error => 'a = "%identity";

let fromPromise = promise => {
  Future.make(resolve => {
    let _ =
      promise
      |> Js.Promise.catch(reason => {
           let error = Error(reason->errorToAny);
           resolve(error);
           Js.Promise.reject(anyToExn(reason));
         })
      |> Js.Promise.then_(value => {
           let value = Ok(value);
           resolve(value);
           Js.Promise.resolve(value);
         });
    None;
  });
};

let fromPromiseUnsafe = promise => {
  Future.make(resolve => {
    let _ =
      promise
      |> Js.Promise.then_(value => {
           resolve(value);
           Js.Promise.resolve(value);
         });
    None;
  });
};

let toPromise = future => {
  Js.Promise.make((~resolve, ~reject as _) => {
    future->Future.get(value => {resolve(. value)})
  });
};

let toPromiseResult = future => {
  Js.Promise.make((~resolve, ~reject) => {
    future->Future.get(value => {
      switch (value) {
      | Ok(value) => resolve(. value)
      | Error(error) => reject(. anyToExn(error))
      }
    })
  });
};
