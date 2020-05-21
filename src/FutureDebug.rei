let tap: Future.t('a) => Future.t('a);

let tapOk: Future.t(result('a, 'b)) => Future.t(result('a, 'b));

let tapError: Future.t(result('a, 'b)) => Future.t(result('a, 'b));
