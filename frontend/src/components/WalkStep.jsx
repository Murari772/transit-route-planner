import { formatMinutes, formatStationName } from '../utils/formatters'
import { WalkIcon, TransferIcon } from './Icons'

export function WalkStep({ segment }) {
  const isZeroMin = segment.travelTimeMinutes === 0
  const Icon = isZeroMin ? TransferIcon : WalkIcon
  
  return (
    <li className={`step-item line-interchange`}>
      <div className={`step-marker interchange line-interchange`}><Icon /></div>
      <div className={`step-content interchange-content line-interchange`}>
        <div className="step-title">
          {isZeroMin ? 'Transfer' : 'Walk'}
        </div>
        <p>
          {isZeroMin ? 'Change to ' : 'Walk to '}
          <strong>{formatStationName(segment.to)}</strong>.
        </p>
        <div className="step-meta">
          <span>{isZeroMin ? 'Immediate transfer' : `${formatMinutes(segment.travelTimeMinutes)} walk`}</span>
        </div>
      </div>
    </li>
  )
}
