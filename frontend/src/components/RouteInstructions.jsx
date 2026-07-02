import { RideStep } from './RideStep'
import { WalkStep } from './WalkStep'

export function RouteInstructions({ segments }) {
  const steps = segments.reduce(
    (accumulator, segment, index) => {
      const isMajorRide = segment.type === 'ride' && segment.mode !== 'walk'
      const nextRideNumber =
        isMajorRide ? accumulator.rideStepNumber + 1 : accumulator.rideStepNumber

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
            if (segment.mode === 'walk') {
              return (
                <WalkStep
                  key={`${segment.type}-${segment.from.id}-${segment.to.id}-${index}`}
                  segment={segment}
                  stepNumber={rideStepNumber}
                />
              )
            }
            return (
              <RideStep
                key={`${segment.type}-${segment.from.id}-${segment.to.id}-${index}`}
                segment={segment}
                stepNumber={rideStepNumber}
              />
            )
          }

          return null
        })}
      </ol>
    </div>
  )
}
