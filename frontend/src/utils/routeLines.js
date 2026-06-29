const routeLineColors = [
  { key: 'red', label: 'Red Line', aliases: ['red'] },
  { key: 'yellow', label: 'Yellow Line', aliases: ['yellow'] },
  { key: 'blue', label: 'Blue Line', aliases: ['blue'] },
  { key: 'green', label: 'Green Line', aliases: ['green'] },
  { key: 'violet', label: 'Violet Line', aliases: ['violet', 'purple'] },
  { key: 'pink', label: 'Pink Line', aliases: ['pink'] },
  { key: 'magenta', label: 'Magenta Line', aliases: ['magenta'] },
  { key: 'orange', label: 'Orange Line', aliases: ['orange', 'airport'] },
  { key: 'grey', label: 'Grey Line', aliases: ['grey', 'gray'] },
  { key: 'aqua', label: 'Aqua Line', aliases: ['aqua'] },
  { key: 'bus', label: 'Bus Route', aliases: ['bus'] },
]

export function getRouteLine(route = '', mode = '') {
  const normalized = `${route} ${mode}`.toLowerCase()
  const match = routeLineColors.find((line) =>
    line.aliases.some((alias) => normalized.includes(alias)),
  )

  return match ?? { key: 'default', label: 'Transit Route' }
}

export function getRouteLineClass(route, mode) {
  return `line-${getRouteLine(route, mode).key}`
}
