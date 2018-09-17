open BsRecharts;

let component = ReasonReact.statelessComponent("SomeComponent");

let make = (~data, _children) => {
  ...component,
  render: (_self) =>
    <div>
      <ResponsiveContainer height=(Px(200.))>
        <BarChart data>

            <Bar name="The types" dataKey="types" fill="#2078b4" stackId="a" />
            <Bar name="The clickers" dataKey="clicks" fill="#ff7f02" stackId="b" />
            <Tooltip />
            <YAxis />
            <CartesianGrid strokeDasharray="3 3" />
          </BarChart>
          /* <Legend align=`left iconType=`circle /> */
      </ResponsiveContainer>
      <ResponsiveContainer height=(Px(100.))>
        <AreaChart data>
          <Area _type=`monotone name="How good?" dataKey="ratio" />
          <YAxis dataKey="ratio" name="Quality" unit="%" />
          <Tooltip />
          <CartesianGrid strokeDasharray="3 3" />
        </AreaChart>
      </ResponsiveContainer>
    </div>
};