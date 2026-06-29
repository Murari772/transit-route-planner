export function RouteNotice({ message }) {
  if (!message) return null

  return (
    <section className="notice error" role="alert">
      <strong>Could not find a route</strong>
      <span>{message}</span>
    </section>
  )
}
