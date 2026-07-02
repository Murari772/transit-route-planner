export function formatMinutes(value, isRide = false) {
  const num = Number(value) || 0
  let minutes = Math.round(num)
  if (!isRide && num > 0 && num < 1.5) {
    minutes = 1
  }
  minutes = Math.max(isRide ? 1 : 0, minutes)
  return `${minutes} min`
}

export function formatMode(mode) {
  if (!mode) return 'Transit'
  return mode.charAt(0).toUpperCase() + mode.slice(1)
}

export function formatRouteName(route) {
  if (!route || route === 'unknown') return 'route'
  return route
}

export function normalizeStation(value, stations) {
  const trimmed = value.trim()
  return stations.find((station) => station.toLowerCase() === trimmed.toLowerCase()) ?? trimmed
}

export function formatStationName(station) {
  if (!station || !station.id) return station?.name || ''
  
  // IDFM is Île-de-France Mobilités (Paris), their station names usually don't need ' Metro' appended
  if (station.id.includes('IDFM')) {
    return station.name
  }

  if (station.id.startsWith('metro_')) {
    return `${station.name} Metro`
  }
  if (station.id.startsWith('bus_')) {
    return `${station.name} Bus Stop`
  }
  return station.name
}
