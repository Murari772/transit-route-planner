const routeLineColors = [
  { key: 'red', label: 'Red Line', aliases: ['red', 'r_'] },
  { key: 'yellow', label: 'Yellow Line', aliases: ['yellow', 'y_'] },
  { key: 'blue', label: 'Blue Line', aliases: ['blue', 'b_'] },
  { key: 'green', label: 'Green Line', aliases: ['green', 'g_ib', 'g_kb'] },
  { key: 'violet', label: 'Violet Line', aliases: ['violet', 'purple', 'v_'] },
  { key: 'pink', label: 'Pink Line', aliases: ['pink', 'p_'] },
  { key: 'magenta', label: 'Magenta Line', aliases: ['magenta', 'm_'] },
  { key: 'orange', label: 'Orange Line', aliases: ['orange', 'airport', 'o_'] },
  { key: 'grey', label: 'Grey Line', aliases: ['grey', 'gray', 'g_dd'] },
  { key: 'aqua', label: 'Aqua Line', aliases: ['aqua', 'a_'] },
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
