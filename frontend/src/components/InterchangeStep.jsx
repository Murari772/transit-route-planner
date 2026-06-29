import { formatMinutes, formatMode } from '../utils/formatters'
import { RouteBadge } from './RouteBadge'

export function InterchangeStep({ segment }) {
  const interchangeClass = 'line-interchange'

  return (
    <li className={`step-item ${interchangeClass}`}>
      <div className={`step-marker interchange ${interchangeClass}`}>↻</div>
      <div className={`step-content interchange-content ${interchangeClass}`}>
        <div className="step-title">Transfer at {segment.atStation}</div>
        <p>
          Move from {' '}
          <RouteBadge route={segment.fromRoute} mode={segment.fromMode} /> to{' '}
           <RouteBadge route={segment.toRoute} mode={segment.toMode} />.
        </p>
        <div className="step-meta">
          <span>{formatMinutes(segment.transferTimeMinutes)} transfer</span>
        </div>
      </div>
    </li>
  )
}
