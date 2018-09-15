[%bs.raw {|require('./App.css')|}];
Node.Fs.readFileSync("~/.momentum/latest", `utf8);


let component = ReasonReact.statelessComponent("App");

let make = (~message, _children) => {
  ...component,
  render: _self =>
    <div className="App">
      <div className="">
        <h2> (ReasonReact.string(message)) </h2>
      </div>
      <p className="App-intro" />
    </div>,
};
