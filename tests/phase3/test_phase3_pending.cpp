#include <gtest/gtest.h>

// Phase 3 Qt UI is not implemented yet.
// Prefer testing controllers/presenters (no QWidget) once they exist.
// Full widget tests can use Qt Test later if needed.

TEST(Phase3AuthControllerTest, DISABLED_Login_EmitsSucceededOnValidCredentials) {
    GTEST_SKIP() << "Implement AuthController::login + signals";
}

TEST(Phase3AuthControllerTest, DISABLED_Login_EmitsFailedOnInvalidCredentials) {
    GTEST_SKIP() << "Implement AuthController::login failure signal";
}

TEST(Phase3AuthControllerTest, DISABLED_RegisterUser_EmitsSucceeded) {
    GTEST_SKIP() << "Implement AuthController::registerUser";
}

TEST(Phase3OrderControllerTest, DISABLED_PlaceMarketOrder_EmitsAccepted) {
    GTEST_SKIP() << "Implement OrderController::placeMarketOrder success path";
}

TEST(Phase3OrderControllerTest, DISABLED_PlaceMarketOrder_EmitsRejected) {
    GTEST_SKIP() << "Implement OrderController::placeMarketOrder reject path";
}

TEST(Phase3PortfolioPresentationTest, DISABLED_Refresh_MapsPortfolioDto) {
    GTEST_SKIP() << "Implement portfolio DTO mapping / table model refresh";
}
