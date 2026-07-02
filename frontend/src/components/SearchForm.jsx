import { routeCriteria } from '../constants'

export function SearchForm({
  city,
  availableCities,
  onCityChange,
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
      {availableCities.length > 0 && (
        <div className="city-selector">
          <label className="field">
            <span>City</span>
            <select value={city} onChange={(e) => onCityChange(e.target.value)}>
              {availableCities.map((c) => (
                <option key={c} value={c}>
                  {c}
                </option>
              ))}
            </select>
          </label>
        </div>
      )}

      <div className="station-grid">
        <label className="field">
          <span>From</span>
          <input
            type="text"
            list={`stations-${city}`}
            value={source}
            onChange={(e) => onSourceChange(e.target.value)}
            placeholder="Starting station"
            autoComplete="off"
            disabled={!city || stationsStatus !== 'ready'}
          />
        </label>

        <button
          className="swap-button"
          type="button"
          onClick={onSwapStations}
          aria-label="Swap source and destination"
          title="Swap stations"
          disabled={!city || stationsStatus !== 'ready'}
        >
          ⇄
        </button>

        <label className="field">
          <span>To</span>
          <input
            type="text"
            list={`stations-${city}`}
            value={destination}
            onChange={(e) => onDestinationChange(e.target.value)}
            placeholder="Destination station"
            autoComplete="off"
            disabled={!city || stationsStatus !== 'ready'}
          />
        </label>
      </div>

      <datalist id={`stations-${city}`}>
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
