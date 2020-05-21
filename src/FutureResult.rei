let getOk: (Future.t(result('a, 'b)), 'a => unit) => unit;

let mapOk:
  (Future.t(result('a, 'b)), 'a => 'c) => Future.t(result('c, 'b));

let flatMapOk:
  (Future.t(result('a, 'b)), 'a => Future.t(result('c, 'b))) =>
  Future.t(result('c, 'b));

let getError: (Future.t(result('a, 'b)), 'b => unit) => unit;

let mapError:
  (Future.t(result('a, 'b)), 'b => 'c) => Future.t(result('a, 'c));

let flatMapError:
  (Future.t(result('a, 'b)), 'b => Future.t(result('a, 'c))) =>
  Future.t(result('a, 'c));
