#include "map_renderer.h"
#include <sstream>

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

void MapRenderer::DrawLines(SphereProjector& proj) {
	int color_index = 0;
	size_t max_color_index = properties_.color_palette.size();

	for (const auto& bus : handler_.GetAllBuses()) {
		if (color_index == (int)max_color_index) {
			color_index = 0;
		}
		std::vector<geo::Coordinates> geo_coords;
		svg::Polyline line;

		for (const auto& stop : bus.stops) {
			geo_coords.push_back(stop->coordinates);
		}

		for (const auto& coord : geo_coords) {
			line.AddPoint(proj(coord));
		}

		document_.Add(line.SetFillColor("none")
			.SetStrokeColor(properties_.color_palette[color_index])
			.SetStrokeWidth(properties_.line_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
		color_index++;
	}
}

void MapRenderer::DrawBusNames(SphereProjector& proj) {
	int color_index = 0;
	size_t max_color_index = properties_.color_palette.size();
	for (const auto& bus : handler_.GetAllBuses()) {
		if (color_index == (int)max_color_index) {
			color_index = 0;
		}
		svg::Text bus_name;
		bus_name.SetData(bus.bus_num);
		bus_name.SetOffset({ properties_.bus_label_offset[0], properties_.bus_label_offset[1] });
		bus_name.SetFontSize(properties_.bus_label_font_size);
		bus_name.SetFontFamily("Verdana");
		bus_name.SetFontWeight("bold");
		bus_name.SetPosition({ proj(bus.stops[0]->coordinates) });
		bus_name.SetFillColor(properties_.color_palette[color_index]);

		svg::Text bus_background = bus_name;
		bus_background.SetFillColor(properties_.underlayer_color)
			.SetStrokeColor(properties_.underlayer_color)
			.SetStrokeWidth(properties_.underlayer_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		
		document_.Add(bus_background);
		document_.Add(bus_name);

		if (!bus.is_roundtrip && (bus.stops[bus.stops.size() / 2] != bus.stops[0])) {
			svg::Text bus_name_end = bus_name;
			bus_name_end.SetPosition({ proj(bus.stops[bus.stops.size() / 2]->coordinates) });

			svg::Text bus_background_end = bus_name_end;
			bus_background_end.SetFillColor(properties_.underlayer_color)
				.SetStrokeColor(properties_.underlayer_color)
				.SetStrokeWidth(properties_.underlayer_width)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			
			document_.Add(bus_background_end);
			document_.Add(bus_name_end);
		}
		color_index++;
	}
}

void MapRenderer::DrawStops(SphereProjector& proj) {
	svg::Circle circle;
	circle.SetFillColor("white")
		.SetRadius(properties_.stop_radius);
	for (const auto& coord : coordinates_) {
		circle.SetCenter({ proj(coord) });
		document_.Add(circle);
	}
}

void MapRenderer::DrawStopNames(SphereProjector& proj) {
	for (const auto& stop : handler_.GetAllStops()) {
		svg::Text stop_name;
		stop_name.SetData(stop.name)
			.SetOffset({ properties_.stop_label_offset[0], properties_.stop_label_offset[1] })
			.SetFontSize(properties_.stop_label_font_size)
			.SetFontFamily("Verdana")
			.SetPosition({ proj(stop.coordinates) })
			.SetFillColor("black");

		svg::Text stop_background = stop_name;
		stop_background.SetFillColor(properties_.underlayer_color)
			.SetStrokeColor(properties_.underlayer_color)
			.SetStrokeWidth(properties_.underlayer_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

		document_.Add(stop_background);
		document_.Add(stop_name);
	}
}

void MapRenderer::DrawMap(std::ostringstream& out) {
	for (const auto& stop : handler_.GetAllStops()) {
		coordinates_.push_back(stop.coordinates);
	}

	SphereProjector proj{ coordinates_.begin(),
						coordinates_.end(),
						properties_.width,
						properties_.height,
						properties_.padding };

	DrawLines(proj);
	DrawBusNames(proj);
	DrawStops(proj);
	DrawStopNames(proj);
	document_.Render(out);
}

MapRenderer& MapRenderer::SetWidth(double width) {
	properties_.width = std::move(width);
	return *this;
}

MapRenderer& MapRenderer::SetHeight(double height) {
	properties_.height = std::move(height);
	return *this;
}

MapRenderer& MapRenderer::SetPadding(double padding) {
	properties_.padding = std::move(padding);
	return *this;
}

MapRenderer& MapRenderer::SetLineWidth(double line_width) {
	properties_.line_width = std::move(line_width);
	return *this;
}

MapRenderer& MapRenderer::SetStopRadius(double stop_radius) {
	properties_.stop_radius = std::move(stop_radius);
	return *this;
}

MapRenderer& MapRenderer::SetBusLabelFontSize(int label_size) {
	properties_.bus_label_font_size = std::move(label_size);
	return *this;
}

MapRenderer& MapRenderer::SetBusLabelOffset(std::array<double, 2> offset) {
	properties_.bus_label_offset = std::move(offset);
	return *this;
}

MapRenderer& MapRenderer::SetStopLabelFontSize(int label_size) {
	properties_.stop_label_font_size = std::move(label_size);
	return *this;
}

MapRenderer& MapRenderer::SetStopLabelOffset(std::array<double, 2> offset) {
	properties_.stop_label_offset = std::move(offset);
	return *this;
}

MapRenderer& MapRenderer::SetUnderlayerColor(svg::Color color) {
	properties_.underlayer_color = std::move(color);
	return *this;
}

MapRenderer& MapRenderer::SetUnderlayerWidth(double width) {
	properties_.underlayer_width = std::move(width);
	return *this;
}

MapRenderer& MapRenderer::SetColorPalette(std::vector<svg::Color> color_palette) {
	properties_.color_palette = std::move(color_palette);
	return *this;
}