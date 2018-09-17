/* pyshell.run('hello.py',  function  (err, results)  {
    if  (err)  throw err;
    console.log('hello.py finished.');
    console.log('results', results);
   }); */
[@bs.module "pyshell"] external run : unit => string = "run";

let eventResolutionSeconds = 300.;

let rangeHours = 6;

type entry = {
  keyCode: int,
  timestamp: float
};

let arrayMaxF = Array.fold_left(max, min_float);

let arrayMaxI = Array.fold_left(max, min_int);

let arrayMinF = Array.fold_left(min, max_float);

let arrayMinI = Array.fold_left(min, max_int);

let deltas_to_timestamps =
  Array.fold_left(
    (acc, e) => {
      let lastTimestamp =
        switch (Array.length(acc)) {
        | 0 => 0.
        | i => acc[i - 1].timestamp
        };
      let timestamp =
        switch e.timestamp {
        | t when t > 1537000000000. => t
        | t => t +. lastTimestamp
        };
      let next: entry = {keyCode: e.keyCode, timestamp};
      Array.append(acc, [|next|])
    },
    [||]
  );

let csv_to_entries =
  Array.map(
    (a) => {
      let entryArray = Js.String.split(",", a);
      switch (Array.length(entryArray)) {
      | 2 => {keyCode: int_of_string(entryArray[0]), timestamp: float_of_string(entryArray[1])}
      | _ => {keyCode: 0, timestamp: 0.}
      }
    }
  );

type bucket = {
  keys: array(int),
  start: float
};

let emptyBucket: bucket = {start: 0., keys: [||]};

[@bs.val] external alert : string => unit = "alert";

let formatTimestamp = (x) => {
  let date = Js.Date.fromFloat(x);
  let len = Js.String.length(Js.Date.toDateString(date));
  let date_str = Js.String.substring(Js.Date.toDateString(date), ~from=0, ~to_=len - 5);
  date_str ++ " " ++ string_of_int(int_of_float(Js.Date.getHours(date))) ++ ":00"
};

let bucketEntries = (entries) => {
  let timestamps = Array.map((s) => s.timestamp, entries);
  Js.Console.log(timestamps);
  let minTimestamp = arrayMinF(timestamps);
  let maxTimestamp = arrayMaxF(timestamps);
  let get_timestamp_index = (timestamp) =>
    int_of_float((timestamp -. minTimestamp) /. eventResolutionSeconds /. 1000.);
  let bucketCount = get_timestamp_index(maxTimestamp);
  let step = (maxTimestamp -. minTimestamp) /. float_of_int(bucketCount);
  let buckets =
    Array.mapi(
      (index, _bucket) => {start: minTimestamp +. step *. float_of_int(index), keys: [||]},
      Array.make(bucketCount + 1, emptyBucket)
    );
  let result =
    Array.fold_left(
      (distribution, entry) => {
        let index = get_timestamp_index(entry.timestamp);
        if (index < Array.length(distribution)) {
          distribution[index] = {
            start: entry.timestamp,
            keys: Array.append(distribution[index].keys, [|entry.keyCode|])
          }
        };
        distribution
      },
      buckets,
      entries
    );
  result
};

let metas = [15, 42, 3675, 54, 56, 29, 3640, 3676];

let filterEntries = (blacklist, entries) =>
  Belt_Array.keep(entries, (e) => ! Belt_List.some(blacklist, (k) => k == e.keyCode));

let get_ranges = (buckets) => {
  let rangeSize = rangeHours * 60 * 60 / int_of_float(eventResolutionSeconds);
  let indices = ref([||]);
  let count = ref(Array.length(buckets));
  while (count^ > 0) {
    let next = arrayMaxI([|0, count^ - rangeSize|]);
    indices := Array.append(indices^, [|(next, count^ - next)|]);
    count := next
  };
  Js.log(indices^);
  let ranges =
    indices^
    |> Array.fold_left(
         (arr, (offset, len)) =>
           Array.append(
             arr,
             [|Belt.Array.reverse(Belt.Array.slice(Belt.Array.reverse(buckets), ~offset, ~len))|]
           ),
         [||]
       )
    |> Belt.Array.reverse;
  Belt.Array.keep(
    ranges,
    (range) => Belt.Array.some(range, (bucket) => Array.length(bucket.keys) > 0)
  )
};

let processStats = (contents) => {
  let entries =
    contents
    |> Js.String.split("\n")
    |> csv_to_entries
    |> deltas_to_timestamps
    |> filterEntries(metas);
  bucketEntries(entries)
};

let get_bucketized_chart =
  Array.map(
    (b) => {
      let clicks = Belt_Array.keep(b.keys, (k) => k == (-1));
      let clickCount = Array.length(clicks);
      let typeCount = Array.length(b.keys);
      {
        "name": b.start,
        "clicks": clickCount,
        "types": typeCount,
        "ratio": 100 * typeCount / (clickCount + typeCount + 10)
      }
    }
  );