export function LoadingState() {
  return (
    <section className="loading-state" aria-label="Loading route details">
      <div className="spinner"></div>
      <h2>Finding the best route...</h2>
      <p>Analyzing transit networks to optimize your journey.</p>
    </section>
  )
}
