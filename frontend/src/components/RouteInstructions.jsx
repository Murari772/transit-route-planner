import { InterchangeStep } from './InterchangeStep'
import { RideStep } from './RideStep'

export function RouteInstructions({ segments }) {
  const steps = segments.reduce(
    (accumulator, segment, index) => {
      const nextRideNumber =
        segment.type === 'ride' ? accumulator.rideStepNumber + 1 : accumulator.rideStepNumber

      accumulator.items.push({
        index,
        segment,
        rideStepNumber: nextRideNumber,
      })

      return {
        items: accumulator.items,
        rideStepNumber: nextRideNumber,
      }
    },
    { items: [], rideStepNumber: 0 },
  ).items

  return (
    <div className="instructions">
      <h3>Route instructions</h3>
      <ol className="step-list">
        {steps.map(({ segment, index, rideStepNumber }) => {
          if (segment.type === 'ride') {
            return (
              <RideStep
                key={`${segment.type}-${segment.from}-${segment.to}-${index}`}
                segment={segment}
                stepNumber={rideStepNumber}
              />
            )
          }

          return (
            <InterchangeStep
              key={`${segment.type}-${segment.atStation}-${index}`}
              segment={segment}
            />
          )
        })}
      </ol>
    </div>
  )
}
