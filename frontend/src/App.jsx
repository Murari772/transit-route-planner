import { useState, useEffect } from 'react'
import './App.css'

function App() {
  const [source, setSource] = useState("");
  const [destination, setDestination] = useState("");
  const [criterion, setCriterion] = useState("");
  const [route, setRoute] = useState(null);
  const [stations, setStations] = useState([]);
  const [error, setError] = useState(null);

  const handleSearch = async () => {
    setError(null);
    setRoute(null);
    try {
      const actualSource = stations.find(s => s.toLowerCase() === source.toLowerCase()) || source;
      const actualDestination = stations.find(s => s.toLowerCase() === destination.toLowerCase()) || destination;

      const response = await fetch(
        `http://localhost:8080/route?source=${encodeURIComponent(actualSource)}&destination=${encodeURIComponent(actualDestination)}&criterion=${encodeURIComponent(criterion)}`
      );
      
      if (!response.ok) {
        setError("Station does not exist or route not found");
        return;
      }

      const data = await response.json();

      setRoute(data);

    } catch (err) {
      console.error("Error: ", err);
      setError("Station does not exist or route not found");
    }
  }

  useEffect(() => {
    fetch("http://localhost:8080/stations") 
      .then(res => res.json())
      .then(data => setStations(data))
      .catch(err => console.error(err));
  }, []);

  return (
    <div>
      <input
        type="text"
        list="stations"
        value={source}
        onChange = {(e) => setSource(e.target.value)}
        placeholder="Enter source"
      />

      <input
        type="text"
        list="stations"
        value={destination}
        onChange = {(e) => setDestination(e.target.value)}
        placeholder="Enter destination"
      />

      <datalist id="stations">
        {stations.map((station) => (
          <option key={station} value={station} />
        ))}
      </datalist>

      <select
        value={criterion}
        onChange={(e) => setCriterion(e.target.value)}
      >
        <option value="">Select an option</option>
        <option value="least_time">Least Time</option>
        <option value="least_interchanges">Least Interchanges</option>
      </select>

      <button onClick={handleSearch}>
        Find Route
      </button>

      {error && <div>{error}</div>}

      {route && !error && (
        <div>
          <h2>Route</h2>

          <p>
            Total Time: {route.path.totalTravelTimeMinutes} minutes
          </p>

          <p>
            Number of Interchanges: {route.path.numberOfInterchanges}
          </p>

          <ul>
            {route.path.segments.map((segment, index) => (
              <li key={index}>
                {segment.type === "ride" ? (
                  <>
                    {segment.from} → {segment.to}
                    {" | "}
                    {segment.mode}
                    {" | "}
                    {segment.route}
                    {" | "}
                    {segment.travelTimeMinutes} min

                    {segment.intermediateStations.length > 0 && (
                      <>
                        {" | Stops: "}
                        {segment.intermediateStations.join(", ")}
                      </>
                    )}
                  </>
                ) : (
                  <>
                    Interchange at {segment.atStation}:
                    {" "}
                    {segment.fromMode} ({segment.fromRoute})
                    {" → "}
                    {segment.toMode} ({segment.toRoute})
                    {" | "}
                    {segment.transferTimeMinutes} min
                  </>
                )}
              </li>
            ))}
          </ul>
        </div>
      )}
    </div>

  )
}

export default App
