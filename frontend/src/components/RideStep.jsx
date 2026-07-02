import { formatMinutes, formatMode, formatRouteName, formatStationName } from '../utils/formatters'
import { getRouteLine } from '../utils/routeLines'
import { RouteBadge } from './RouteBadge'
import { BusIcon, TrainIcon } from './Icons'

export function RideStep({ segment, stepNumber }) {
  const line = getRouteLine(segment.route, segment.mode)
  const lineClass = `line-${line.key}`
  const stops = segment.intermediateStations ?? []

  const displayRouteName = line.key === 'default' || line.key === 'bus' ? formatRouteName(segment.route) : line.label
  const Icon = segment.mode === 'bus' ? BusIcon : TrainIcon

  return (
    <li className={`step-item ${lineClass}`}>
      <div className={`step-marker ride ${lineClass}`}>
        <Icon />
      </div>
      <div className={`step-content ${lineClass}`}>
        <div className="step-title">
          Ride {formatMode(segment.mode)} <RouteBadge route={segment.route} mode={segment.mode} />
        </div>
        <p>
          Board at <strong>{formatStationName(segment.from)}</strong> and get down at{' '}
          <strong>{formatStationName(segment.to)}</strong>.
        </p>
        <div className="step-meta">
          {segment.waitTimeMinutes > 0 && (
            <span>Wait: {formatMinutes(segment.waitTimeMinutes)}</span>
          )}
          <span>{formatMinutes(segment.travelTimeMinutes, true)}</span>
          <span>{displayRouteName}</span>
          {stops.length > 0 && (
            <span>
              {stops.length} intermediate stop{stops.length === 1 ? '' : 's'}
            </span>
          )}
        </div>
        {stops.length > 0 && (
          <details className="stops-details">
            <summary>View stops on this line</summary>
            <ol className="stop-list">
              {stops.map((station) => (
                <li key={station.id}>{station.name}</li>
              ))}
            </ol>
          </details>
        )}
      </div>
    </li>
  )
}
