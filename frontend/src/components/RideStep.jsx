import { formatMinutes, formatMode, formatRouteName } from '../utils/formatters'
import { getRouteLineClass } from '../utils/routeLines'
import { RouteBadge } from './RouteBadge'

export function RideStep({ segment, stepNumber }) {
  const lineClass = getRouteLineClass(segment.route, segment.mode)
  const stops = segment.intermediateStations ?? []

  return (
    <li className={`step-item ${lineClass}`}>
      <div className={`step-marker ride ${lineClass}`}>{stepNumber}</div>
      <div className={`step-content ${lineClass}`}>
        <div className="step-title">
          Ride {formatMode(segment.mode)} <RouteBadge route={segment.route} mode={segment.mode} />
        </div>
        <p>
          Board at <strong>{segment.from}</strong> and get down at{' '}
          <strong>{segment.to}</strong>.
        </p>
        <div className="step-meta">
          <span>{formatMinutes(segment.travelTimeMinutes)}</span>
          <span>{formatRouteName(segment.route)}</span>
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
                <li key={station}>{station}</li>
              ))}
            </ol>
          </details>
        )}
      </div>
    </li>
  )
}
