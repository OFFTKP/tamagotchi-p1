#include "frontend.hxx"
#include "imgui/imgui.h"

Frontend::Frontend() {
    glCreateTextures(GL_TEXTURE_2D, 1, &texture_);
    glTextureParameteri(texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(texture_, 1, GL_RGB8, 32, 16);
}

void Frontend::Draw() {
    for (int i = 0; i < CpuFrequency; i++)
        tama_.Step();
    tama_.KeyRelease(0);
    tama_.KeyRelease(1);
    tama_.KeyRelease(2);
    const auto& display = tama_.GetDisplay();
    glTextureSubImage2D(texture_, 0, 0, 0, 32, 16, GL_LUMINANCE, GL_UNSIGNED_BYTE, display.data());
    ImGui::Begin("Tama");
    ImGui::Image((ImTextureID)texture_, ImVec2(256, 128));
    if (ImGui::Button("A")) {
        tama_.KeyPress(2);
    }
    if (ImGui::Button("B")) {
        tama_.KeyPress(1);
    }
    if (ImGui::Button("C")) {
        tama_.KeyPress(0);
    }
    ImGui::End();
}

void Frontend::Load(const std::string& filename) {
    tama_.LoadRom(filename);
    for (int i = 0; i < 0x10000; i++) {
        tama_.Step();
    }
}