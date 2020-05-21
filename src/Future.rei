type cancellationToken;

type cancelFunction = unit => unit;

type futureCallback('value) = 'value => unit;

type resolve('value) = 'value => unit;

type setup('value) = resolve('value) => option(cancelFunction);

type t('value);

let make: setup('value) => t('value);

type deferred('value) = {
  future: t('value),
  resolve: resolve('value),
};

let deferred: unit => deferred('a);

let value: 'a => t('a);

let get: (t('a), 'a => unit) => unit;

let cancel: t('a) => unit;

let map: (t('a), ~propagateCancel: bool=?, 'a => 'b) => t('b);

let flatMap: (t('a), ~propagateCancel: bool=?, 'a => t('b)) => t('b);

let all: array(t('a)) => t(array('a));

let all2: (t('a), t('b)) => t(('a, 'b));

let all3: (t('a), t('b), t('c)) => t(('a, 'b, 'c));

let all4: (t('a), t('b), t('c), t('d)) => t(('a, 'b, 'c, 'd));

let all5:
  (t('a), t('b), t('c), t('d), t('e)) => t(('a, 'b, 'c, 'd, 'e));
