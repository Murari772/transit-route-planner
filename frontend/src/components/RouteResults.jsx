import { routeCriteria } from '../constants'
import { RouteInstructions } from './RouteInstructions'
import { RouteSummary } from './RouteSummary'
import { StationSequence } from './StationSequence'

export function RouteResults({ route }) {
  const preferenceLabel =
    routeCriteria.find((item) => item.value === route.criterion)?.label ?? 'Best route'

  return (
    <section className="results-panel" aria-labelledby="results-title">
      <div className="results-header">
        <div>
          <p className="eyebrow">Suggested route</p>
          <h2 id="results-title">
            {route.source.name} to {route.destination.name}
          </h2>
        </div>
        <span className="preference-pill">{preferenceLabel}</span>
      </div>

      <RouteSummary route={route} />
      <RouteInstructions segments={route.path.segments} />
      <StationSequence stations={route.path.stations} />
    </section>
  )
}
