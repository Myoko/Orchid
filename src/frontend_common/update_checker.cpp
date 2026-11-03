// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <array>
#include <optional>
#include <string_view>

#include "common/logging.h"
#include "common/net/net.h"
#include "common/scm_rev.h"
#include "update_checker.h"

namespace UpdateChecker {

namespace {

constexpr u32 max_changes = 100;

// Release tags are v<major>.<minor>.<patch>, plus -<channel> on every channel but stable, so
// stable v0.4.2 and experimental v0.4.2-experimental never offer each other as updates.
struct BuildTag {
    std::string_view channel;
    std::array<u64, 3> version;
};

constexpr std::optional<BuildTag> ParseBuildTag(std::string_view tag) {
    if (tag.size() < 2 || tag[0] != 'v')
        return std::nullopt;
    tag.remove_prefix(1);

    BuildTag result{"stable", {}};
    if (const auto dash = tag.find('-'); dash != std::string_view::npos) {
        result.channel = tag.substr(dash + 1);
        tag = tag.substr(0, dash);
        if (result.channel.empty())
            return std::nullopt;
    }

    size_t part = 0;
    bool digit = false;
    for (const char c : tag) {
        if (c == '.') {
            if (!digit || ++part == result.version.size())
                return std::nullopt;
            digit = false;
        } else if (c >= '0' && c <= '9') {
            result.version[part] = result.version[part] * 10 + static_cast<u64>(c - '0');
            digit = true;
        } else {
            return std::nullopt;
        }
    }
    if (!digit || part != result.version.size() - 1)
        return std::nullopt;
    return result;
}

static_assert(ParseBuildTag("v0.4.12")->channel == "stable");
static_assert(ParseBuildTag("v0.4.12")->version == std::array<u64, 3>{0, 4, 12});
static_assert(ParseBuildTag("v0.4.0-experimental")->channel == "experimental");
static_assert(ParseBuildTag("v1.10.3")->version > ParseBuildTag("v1.9.30")->version);
static_assert(!ParseBuildTag("mario-tennis-fever-build-7"));
static_assert(!ParseBuildTag("v0.4"));
static_assert(!ParseBuildTag("v0..4"));
static_assert(!ParseBuildTag("v0.4.1-"));
static_assert(!ParseBuildTag(""));

} // namespace

Result Check() {
    const auto current = ParseBuildTag(Common::g_build_release_tag);
    if (!current)
        return Failure::NotReleaseBuild;

    const auto releases = Common::Net::GetReleases();
    if (!releases)
        return Failure::Unreachable;

    std::optional<Common::Net::Release> latest;
    auto latest_version = current->version;
    for (const auto& release : *releases) {
        const auto tag = ParseBuildTag(release.tag);
        if (tag && tag->channel == current->channel && tag->version > latest_version) {
            latest = release;
            latest_version = tag->version;
        }
    }

    if (!latest)
        return UpToDate{};

    LOG_INFO(Frontend, "Update available: {}", latest->tag);

    Update update{
        .release = *latest,
        .changes = {},
        .changes_complete = false,
        .compare_url = Common::Net::GetCompareUrl(Common::g_scm_rev, latest->commit),
    };

    if (auto commits = Common::Net::GetCommits(latest->commit, max_changes)) {
        for (auto& commit : *commits) {
            if (commit.sha == Common::g_scm_rev) {
                update.changes_complete = true;
                break;
            }
            update.changes.push_back(std::move(commit));
        }
    }

    return update;
}

} // namespace UpdateChecker
