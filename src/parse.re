/* pyshell.run('hello.py',  function  (err, results)  {
    if  (err)  throw err;
    console.log('hello.py finished.');
    console.log('results', results);
   }); */
[@bs.module "pyshell"] external run : unit => string = "run";

let eventResolutionSeconds = 100.;

type entry = {
  keyCode: int,
  timestamp: float
};

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

let bucket_entries = (entries) => {
  let timestamps = Array.map((s) => s.timestamp, entries);
  Js.Console.log(timestamps);
  let minTimestamp = Array.fold_left(min, max_float, timestamps);
  let maxTimestamp = Array.fold_left(max, min_float, timestamps);
  let num_buckets =
    int_of_float((maxTimestamp -. minTimestamp) /. eventResolutionSeconds /. 1000.);
  let get_timestamp_index = (timestamp) =>
    int_of_float((timestamp -. minTimestamp) /. eventResolutionSeconds /. 1000.);
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
      Array.make(num_buckets + 1, emptyBucket),
      entries
    );
  result
};

let processStats = (contents) => {
  let parsedEntries = contents |> Js.String.split("\n") |> csv_to_entries |> deltas_to_timestamps;
  bucket_entries(parsedEntries)
};