import { formatRouteName } from '../utils/formatters'
import { getRouteLine, getRouteLineClass } from '../utils/routeLines'

export function RouteBadge({ route, mode }) {
  const line = getRouteLine(route, mode)

  return (
    <span className={`route-badge ${getRouteLineClass(route, mode)}`}>
      {line.key === 'default' ? formatRouteName(route) : line.label}
    </span>
  )
}
