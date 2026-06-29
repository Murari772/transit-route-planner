import { routeCriteria } from '../constants'

export function SearchForm({
  source,
  destination,
  criterion,
  stations,
  stationsStatus,
  isLoading,
  canSearch,
  onSourceChange,
  onDestinationChange,
  onCriterionChange,
  onSubmit,
  onSwapStations,
}) {
  return (
    <form className="search-card" onSubmit={onSubmit}>
      <div className="station-grid">
        <label className="field">
          <span>From</span>
          <input
            type="text"
            list="stations"
            value={source}
            onChange={(e) => onSourceChange(e.target.value)}
            placeholder="Starting station"
            autoComplete="off"
          />
        </label>

        <button
          className="swap-button"
          type="button"
          onClick={onSwapStations}
          aria-label="Swap source and destination"
          title="Swap stations"
        >
          ⇄
        </button>

        <label className="field">
          <span>To</span>
          <input
            type="text"
            list="stations"
            value={destination}
            onChange={(e) => onDestinationChange(e.target.value)}
            placeholder="Destination station"
            autoComplete="off"
          />
        </label>
      </div>

      <datalist id="stations">
        {stations.map((station) => (
          <option key={station} value={station} />
        ))}
      </datalist>

      <div className="criteria-group" aria-label="Route preference">
        {routeCriteria.map((option) => (
          <label
            className={`criterion-card ${criterion === option.value ? 'selected' : ''}`}
            key={option.value}
          >
            <input
              type="radio"
              name="criterion"
              value={option.value}
              checked={criterion === option.value}
              onChange={(e) => onCriterionChange(e.target.value)}
            />
            <span>{option.label}</span>
            <small>{option.detail}</small>
          </label>
        ))}
      </div>

      <div className="form-footer">
        <p className={`station-status ${stationsStatus}`}>
          {stationsStatus === 'loading' && 'Loading station list...'}
          {stationsStatus === 'ready' && `${stations.length} stations available`}
          {stationsStatus === 'error' && 'Station list unavailable'}
        </p>
        <button className="primary-button" type="submit" disabled={!canSearch}>
          {isLoading ? 'Finding route...' : 'Show route'}
        </button>
      </div>
    </form>
  )
}
