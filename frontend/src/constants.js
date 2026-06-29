export const API_BASE_URL = import.meta.env.VITE_API_BASE_URL ?? 'http://localhost:8080'

export const routeCriteria = [
  {
    value: 'least_time',
    label: 'Fastest route',
    detail: 'Minimizes total travel time',
  },
  {
    value: 'least_interchanges',
    label: 'Fewest changes',
    detail: 'Keeps the journey easier to follow',
  },
]
