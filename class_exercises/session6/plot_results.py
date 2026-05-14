from pathlib import Path


def read_times(path: Path) -> list[float]:
    lines = path.read_text().splitlines()
    return [float(line.strip()) for line in lines if line.strip()]


def scale(value: float, lower: float, upper: float, out_low: float, out_high: float) -> float:
    if upper == lower:
        return (out_low + out_high) / 2
    ratio = (value - lower) / (upper - lower)
    return out_low + ratio * (out_high - out_low)


def main() -> None:
    base_dir = Path(__file__).resolve().parent
    results_path = base_dir / "results.txt"
    output_path = base_dir / "dot_product_results.svg"

    times = read_times(results_path)
    thread_counts = [1, 2, 4, 6, 8, 10, 12, 16, 32]

    if len(times) != len(thread_counts):
        raise ValueError(
            f"Expected {len(thread_counts)} timing values, found {len(times)} in {results_path}"
        )

    width = 900
    height = 520
    left = 85
    right = 35
    top = 55
    bottom = 95
    plot_w = width - left - right
    plot_h = height - top - bottom

    min_time = min(times)
    max_time = max(times)
    y_padding = (max_time - min_time) * 0.1 if max_time > min_time else 0.1
    y_low = min_time - y_padding
    y_high = max_time + y_padding

    points = []
    for t, time_val in zip(thread_counts, times):
        x = scale(float(t), float(min(thread_counts)), float(max(thread_counts)), left, left + plot_w)
        y = scale(time_val, y_low, y_high, top + plot_h, top)
        points.append((x, y, t, time_val))

    polyline_points = " ".join(f"{x:.2f},{y:.2f}" for x, y, _, _ in points)

    y_ticks = 5
    x_ticks = thread_counts
    y_labels = [y_low + (y_high - y_low) * i / y_ticks for i in range(y_ticks + 1)]

    svg_parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
        '<rect x="0" y="0" width="100%" height="100%" fill="white"/>',
        f'<text x="{width / 2}" y="30" text-anchor="middle" font-size="22" font-family="Arial">Dot Product Runtime by Thread Count</text>',
        f'<line x1="{left}" y1="{top + plot_h}" x2="{left + plot_w}" y2="{top + plot_h}" stroke="black" stroke-width="2"/>',
        f'<line x1="{left}" y1="{top}" x2="{left}" y2="{top + plot_h}" stroke="black" stroke-width="2"/>',
    ]

    for y_val in y_labels:
        y = scale(y_val, y_low, y_high, top + plot_h, top)
        svg_parts.append(
            f'<line x1="{left}" y1="{y:.2f}" x2="{left + plot_w}" y2="{y:.2f}" stroke="#d8d8d8" stroke-width="1"/>'
        )
        svg_parts.append(
            f'<text x="{left - 10}" y="{y + 5:.2f}" text-anchor="end" font-size="12" font-family="Arial">{y_val:.3f}</text>'
        )

    for t in x_ticks:
        x = scale(float(t), float(min(thread_counts)), float(max(thread_counts)), left, left + plot_w)
        svg_parts.append(
            f'<line x1="{x:.2f}" y1="{top}" x2="{x:.2f}" y2="{top + plot_h}" stroke="#efefef" stroke-width="1"/>'
        )
        svg_parts.append(
            f'<text x="{x:.2f}" y="{top + plot_h + 25}" text-anchor="middle" font-size="12" font-family="Arial">{t}</text>'
        )

    svg_parts.append(
        f'<polyline fill="none" stroke="#2563eb" stroke-width="3" points="{polyline_points}"/>'
    )

    for x, y, t, time_val in points:
        svg_parts.append(f'<circle cx="{x:.2f}" cy="{y:.2f}" r="4.5" fill="#1d4ed8"/>')
        svg_parts.append(
            f'<text x="{x:.2f}" y="{y - 10:.2f}" text-anchor="middle" font-size="11" font-family="Arial">{time_val:.3f}s</text>'
        )

    svg_parts.append(
        f'<text x="{left + plot_w / 2}" y="{height - 28}" text-anchor="middle" font-size="14" font-family="Arial">Threads</text>'
    )
    svg_parts.append(
        f'<text x="22" y="{top + plot_h / 2}" transform="rotate(-90 22 {top + plot_h / 2})" text-anchor="middle" font-size="14" font-family="Arial">Time (seconds)</text>'
    )
    svg_parts.append("</svg>")

    output_path.write_text("\n".join(svg_parts))
    print(f"Wrote plot to {output_path}")


if __name__ == "__main__":
    main()
