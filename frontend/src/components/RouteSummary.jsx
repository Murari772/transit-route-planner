import { formatMinutes } from '../utils/formatters'

export function RouteSummary({ route }) {
  const stats = [
    {
      label: 'Travel time',
      value: formatMinutes(route.path.totalTravelTimeMinutes),
    },
    {
      label: 'Transfers',
      value: route.path.numberOfInterchanges,
    },
    {
      label: 'Stations',
      value: route.path.stations?.length ?? 0,
    },
  ]

  return (
    <div className="stats-grid">
      {stats.map((stat) => (
        <div className="stat-card" key={stat.label}>
          <span>{stat.label}</span>
          <strong>{stat.value}</strong>
        </div>
      ))}
    </div>
  )
}
