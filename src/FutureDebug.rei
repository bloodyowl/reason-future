let tap: (Future.t('a), 'a => unit) => Future.t('a);

let tapOk:
  (Future.t(result('a, 'b)), 'a => unit) => Future.t(result('a, 'b));

let tapError:
  (Future.t(result('a, 'b)), 'b => unit) => Future.t(result('a, 'b));
