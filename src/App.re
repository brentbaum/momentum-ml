[%bs.raw {|require('./App.css')|}];

[@bs.module "os"] external homedir : unit => string = "homedir";

let home = homedir();

let logging = false;

let renderStats = (stats) =>
  ReasonReact.array(
    Array.map(
      (bucket: Parse.bucket) =>
        <div>
          <div> (ReasonReact.string(Parse.formatTimestamp(bucket.start))) </div>
          <div>
            (
              ReasonReact.string(
                Js.Array.joinWith(",", Array.map(Translate.getCharacter, bucket.keys))
              )
            )
          </div>
        </div>,
      stats
    )
  );

type action =
  | Read
  | Process;

type state = {
  contents: string,
  stats: array(Parse.bucket)
};

let component = ReasonReact.reducerComponent("App");

let make = (~message, _children) => {
  ...component,
  initialState: () => {contents: "", stats: [||]},
  reducer: (action, state) =>
    switch action {
    | Read =>
      ReasonReact.Update({
        ...state,
        contents: Node.Fs.readFileSync(home ++ "/.momentum/logs/latest", `utf8)
      })
    | Process => ReasonReact.Update({...state, stats: Parse.processStats(state.contents)})
    },
  didMount: (self) => {
    self.send(Read);
    self.send(Process)
  },
  render: (self) =>
    <div className="App">
      <div className=""> <h2> (ReasonReact.string(message)) </h2> </div>
      (
        ReasonReact.array(
          Array.map(
            (d: array(Parse.bucket)) =>
              <div className="chart">
                <div className="chart-title">
                  (
                    ReasonReact.string(
                      Parse.formatTimestamp(d[0].start)
                      ++ " - "
                      ++ Parse.formatTimestamp(d[Array.length(d) - 1].start)
                    )
                  )
                </div>
                <Chart data=(Parse.get_bucketized_chart(d)) />
              </div>,
            Parse.get_ranges(self.state.stats)
          )
        )
      )
      <p className="App-intro"> (logging ? renderStats(self.state.stats) : ReasonReact.null) </p>
    </div>
};