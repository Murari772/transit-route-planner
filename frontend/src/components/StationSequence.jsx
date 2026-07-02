export function StationSequence({ stations }) {
  return (
    <div className="station-strip" aria-label="Complete route station sequence">
      <h3>Station sequence</h3>
      <ol className="station-chain">
        {stations.map((station) => (
          <li key={station.id}>{station.name}</li>
        ))}
      </ol>
    </div>
  )
}
