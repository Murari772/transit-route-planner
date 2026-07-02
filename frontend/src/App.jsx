import { useEffect, useState } from 'react'
import { API_BASE_URL } from './constants'
import { EmptyRouteState } from './components/EmptyRouteState'
import { LoadingState } from './components/LoadingState'
import { PlannerIntro } from './components/PlannerIntro'
import { RouteNotice } from './components/RouteNotice'
import { RouteResults } from './components/RouteResults'
import { SearchForm } from './components/SearchForm'
import { normalizeStation } from './utils/formatters'
import './styles/global.css'
import './styles/planner.css'
import './styles/routeResults.css'
import './styles/routeLines.css'

function App() {
  const [city, setCity] = useState('')
  const [availableCities, setAvailableCities] = useState([])
  const [source, setSource] = useState('')
  const [destination, setDestination] = useState('')
  const [criterion, setCriterion] = useState('least_time')
  const [route, setRoute] = useState(null)
  const [stations, setStations] = useState([])
  const [error, setError] = useState(null)
  const [isLoading, setIsLoading] = useState(false)
  const [stationsStatus, setStationsStatus] = useState('loading')

  const canSearch = source.trim() && destination.trim() && !isLoading && city !== ''

  const handleSearch = async (event) => {
    event.preventDefault()
    setError(null)
    setRoute(null)

    if (!source.trim() || !destination.trim()) {
      setError('Enter both source and destination stations.')
      return
    }

    const actualSource = normalizeStation(source, stations)
    const actualDestination = normalizeStation(destination, stations)

    if (actualSource.toLowerCase() === actualDestination.toLowerCase()) {
      setError('Choose two different stations.')
      return
    }

    setIsLoading(true)

    try {
      const params = new URLSearchParams({
        city,
        source: actualSource,
        destination: actualDestination,
        criterion,
      })

      const response = await fetch(`${API_BASE_URL}/route?${params}`)
      const data = await response.json().catch(() => null)

      if (!response.ok || !data?.found) {
        setError(data?.message || data?.error || 'No route found for these stations.')
        return
      }

      setRoute(data)
    } catch (err) {
      console.error('Route search failed:', err)
      setError('Unable to reach the route service. Check that the backend is running.')
    } finally {
      setIsLoading(false)
    }
  }

  const swapStations = () => {
    setSource(destination)
    setDestination(source)
    setRoute(null)
    setError(null)
  }

  const handleCityChange = (newCity) => {
    setCity(newCity)
    setSource('')
    setDestination('')
    setRoute(null)
    setError(null)
    setStations([])
    setStationsStatus('loading')
  }

  useEffect(() => {
    const controller = new AbortController()
    fetch(`${API_BASE_URL}/cities`, { signal: controller.signal })
      .then((res) => res.json())
      .then((data) => {
        setAvailableCities(data || [])
        if (data && data.length > 0) {
          setCity(data[0])
        }
      })
      .catch((err) => {
        if (err.name !== 'AbortError') console.error('Failed to load cities', err)
      })
    return () => controller.abort()
  }, [])

  useEffect(() => {
    if (!city) return

    setStationsStatus('loading')
    const controller = new AbortController()

    fetch(`${API_BASE_URL}/stations?city=${encodeURIComponent(city)}`, { signal: controller.signal })
      .then((res) => {
        if (!res.ok) throw new Error('Station request failed')
        return res.json()
      })
      .then((data) => {
        setStations(Array.isArray(data) ? data.sort((a, b) => a.localeCompare(b)) : [])
        setStationsStatus('ready')
      })
      .catch((err) => {
        if (err.name === 'AbortError') return
        console.error('Station list failed:', err)
        setStationsStatus('error')
      })

    return () => controller.abort()
  }, [city])

  return (
    <main className="app-shell">
      <section className="planner-panel" aria-labelledby="planner-title">
        <PlannerIntro />
        <SearchForm
          city={city}
          availableCities={availableCities}
          onCityChange={handleCityChange}
          source={source}
          destination={destination}
          criterion={criterion}
          stations={stations}
          stationsStatus={stationsStatus}
          isLoading={isLoading}
          canSearch={canSearch}
          onSourceChange={setSource}
          onDestinationChange={setDestination}
          onCriterionChange={setCriterion}
          onSubmit={handleSearch}
          onSwapStations={swapStations}
        />
      </section>

      <RouteNotice message={error} />
      {isLoading && <LoadingState />}
      {!isLoading && !route && !error && <EmptyRouteState />}
      {!isLoading && route && !error && <RouteResults route={route} />}
    </main>
  )
}

export default App
