[%bs.raw {|require('./App.css')|}];

[@bs.module "os"] external homedir : unit => string = "homedir";

let home = homedir();

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
      <p className="App-intro">
        (
          ReasonReact.array(
            Array.map(
              (bucket: Parse.bucket) =>
                <div>
                  <div>
                    (ReasonReact.string(Js.Date.toISOString(Js.Date.fromFloat(bucket.start))))
                  </div>
                  <div>
                    (
                      ReasonReact.string(
                        Js.Array.joinWith(",", Array.map(Translate.getCharacter, bucket.keys))
                      )
                    )
                  </div>
                </div>,
              self.state.stats
            )
          )
        )
      </p>
    </div>
};