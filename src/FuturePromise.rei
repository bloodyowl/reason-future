let fromPromise: Js.Promise.t('value) => Future.t(result('value, 'error));

let fromPromiseUnsafe: Js.Promise.t('value) => Future.t('value);

let toPromise: Future.t('value) => Js.Promise.t('value);

let toPromiseResult:
  Future.t(result('value, 'error)) => Js.Promise.t('value);
