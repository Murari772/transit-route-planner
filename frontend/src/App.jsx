import { useState } from 'react'
import './App.css'

function App() {
  const [source, setSource] = useState("");
  const [destination, setDestination] = useState("");
  const [criterion, setCriterion] = useState("");
  const [route, setRoute] = useState(null);

  const handleSearch = async () => {
    try {
      const response = await fetch(
        `http://localhost:8080/route?source=${encodeURIComponent(source)}
        &destination=${encodeURIComponent(destination)}
        &criterion=${encodeURIComponent(criterion)}`
      );
      
      if (!response.ok) {
        throw new Error("Failed to fetch route");
      }

      const data = await response.json();

      setRoute(data);

    } catch (error) {
      console.error("Error: ", error);
    }
  }

  return (
    <div>
      <input
        type="text"
        value={source}
        onChange = {(e) => setSource(e.target.value)}
        placeholder="Enter source"
      />

      <input
        type="text"
        value={destination}
        onChange = {(e) => setDestination(e.target.value)}
        placeholder="Enter destination"
      />

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

      {route && (
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
