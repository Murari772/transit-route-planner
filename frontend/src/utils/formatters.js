export function formatMinutes(value) {
  const minutes = Math.max(0, Math.round(Number(value) || 0))
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
