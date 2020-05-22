let next = x => Future.value(x + 1);
let numberOfLoops = 10000;
let rec loop = x => {
  next(x)
  ->Future.flatMap(x' =>
      if (x' == numberOfLoops) {
        Future.value(x');
      } else {
        loop(x');
      }
    );
};
loop(0);
